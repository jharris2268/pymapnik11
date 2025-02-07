# -*- coding: utf-8 -*-

###########################################################################
## Python code generated with wxFormBuilder (version 4.2.1-5-gc2f65a65)
## http://www.wxformbuilder.org/
##
## PLEASE DO *NOT* EDIT THIS FILE!
###########################################################################

import wx
import wx.xrc
import wx.stc

import gettext
_ = gettext.gettext

###########################################################################
## Class GuiBase
###########################################################################

class GuiBase ( wx.Frame ):

    def __init__( self, parent ):
        wx.Frame.__init__ ( self, parent, id = wx.ID_ANY, title = wx.EmptyString, pos = wx.DefaultPosition, size = wx.Size( 1024,768 ), style = wx.DEFAULT_FRAME_STYLE|wx.TAB_TRAVERSAL )

        self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )

        self.m_statusBar1 = self.CreateStatusBar( 1, wx.STB_SIZEGRIP, wx.ID_ANY )
        self.m_menubar1 = wx.MenuBar( 0 )
        self.m_menu1 = wx.Menu()
        self.m_menuItem1 = wx.MenuItem( self.m_menu1, wx.ID_ANY, _(u"MyMenuItem"), wx.EmptyString, wx.ITEM_NORMAL )
        self.m_menu1.Append( self.m_menuItem1 )

        self.m_menuItem2 = wx.MenuItem( self.m_menu1, wx.ID_ANY, _(u"MyMenuItem"), wx.EmptyString, wx.ITEM_NORMAL )
        self.m_menu1.Append( self.m_menuItem2 )

        self.m_menuItem3 = wx.MenuItem( self.m_menu1, wx.ID_ANY, _(u"MyMenuItem"), wx.EmptyString, wx.ITEM_NORMAL )
        self.m_menu1.Append( self.m_menuItem3 )

        self.m_menubar1.Append( self.m_menu1, _(u"&File") )

        self.SetMenuBar( self.m_menubar1 )

        gbSizer1 = wx.GridBagSizer( 0, 0 )
        gbSizer1.SetFlexibleDirection( wx.BOTH )
        gbSizer1.SetNonFlexibleGrowMode( wx.FLEX_GROWMODE_SPECIFIED )

        self.m_staticText1 = wx.StaticText( self, wx.ID_ANY, _(u"Carto File"), wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText1.Wrap( -1 )

        gbSizer1.Add( self.m_staticText1, wx.GBPosition( 0, 0 ), wx.GBSpan( 1, 1 ), wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )

        bSizer3 = wx.BoxSizer( wx.HORIZONTAL )

        self.select_carto_button = wx.Button( self, wx.ID_ANY, _(u"Open"), wx.DefaultPosition, wx.DefaultSize, 0 )
        bSizer3.Add( self.select_carto_button, 0, wx.ALL, 5 )

        self.carto_text_entry = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.TE_PROCESS_ENTER )
        bSizer3.Add( self.carto_text_entry, 1, wx.ALL|wx.EXPAND, 5 )


        gbSizer1.Add( bSizer3, wx.GBPosition( 0, 1 ), wx.GBSpan( 1, 3 ), wx.EXPAND, 5 )

        self.m_staticText2 = wx.StaticText( self, wx.ID_ANY, _(u"Table Prefix"), wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText2.Wrap( -1 )

        gbSizer1.Add( self.m_staticText2, wx.GBPosition( 1, 0 ), wx.GBSpan( 1, 1 ), wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )

        bSizer4 = wx.BoxSizer( wx.HORIZONTAL )

        self.tableprefx_text_entry = wx.TextCtrl( self, wx.ID_ANY, _(u"planet_osm"), wx.DefaultPosition, wx.DefaultSize, 0 )
        self.tableprefx_text_entry.SetMinSize( wx.Size( 250,-1 ) )

        bSizer4.Add( self.tableprefx_text_entry, 0, wx.ALL, 5 )

        self.m_checkBox1 = wx.CheckBox( self, wx.ID_ANY, _(u"Splits"), wx.DefaultPosition, wx.DefaultSize, wx.ALIGN_RIGHT )
        bSizer4.Add( self.m_checkBox1, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )

        self.tableprefx_splits_textentry = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.TE_READONLY )
        bSizer4.Add( self.tableprefx_splits_textentry, 3, wx.ALL|wx.EXPAND, 5 )


        gbSizer1.Add( bSizer4, wx.GBPosition( 1, 1 ), wx.GBSpan( 1, 3 ), wx.EXPAND, 5 )

        self.m_staticText4 = wx.StaticText( self, wx.ID_ANY, _(u"Projection"), wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText4.Wrap( -1 )

        gbSizer1.Add( self.m_staticText4, wx.GBPosition( 2, 0 ), wx.GBSpan( 1, 1 ), wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )

        bSizer5 = wx.BoxSizer( wx.HORIZONTAL )

        projection_comboChoices = [ _(u"3857"), _(u"27700"), wx.EmptyString ]
        self.projection_combo = wx.ComboBox( self, wx.ID_ANY, _(u"3857"), wx.DefaultPosition, wx.DefaultSize, projection_comboChoices, 0 )
        self.projection_combo.SetSelection( 0 )
        bSizer5.Add( self.projection_combo, 0, wx.ALL, 5 )

        self.m_staticText6 = wx.StaticText( self, wx.ID_ANY, _(u"Scale Correction"), wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText6.Wrap( -1 )

        bSizer5.Add( self.m_staticText6, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )

        self.scale_correction_textentry = wx.TextCtrl( self, wx.ID_ANY, _(u"1"), wx.DefaultPosition, wx.DefaultSize, 0 )
        bSizer5.Add( self.scale_correction_textentry, 0, wx.ALL, 5 )

        self.mapnik_status_label = wx.StaticText( self, wx.ID_ANY, _(u"Mapnik not initialized"), wx.DefaultPosition, wx.DefaultSize, 0 )
        self.mapnik_status_label.Wrap( -1 )

        bSizer5.Add( self.mapnik_status_label, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )

        self.m_button3 = wx.Button( self, wx.ID_ANY, _(u"Initialize Mapnik"), wx.DefaultPosition, wx.DefaultSize, 0 )
        bSizer5.Add( self.m_button3, 0, wx.ALL, 5 )


        gbSizer1.Add( bSizer5, wx.GBPosition( 2, 1 ), wx.GBSpan( 1, 1 ), wx.EXPAND, 5 )

        self.m_staticline2 = wx.StaticLine( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.LI_HORIZONTAL )
        gbSizer1.Add( self.m_staticline2, wx.GBPosition( 3, 0 ), wx.GBSpan( 1, 4 ), wx.EXPAND |wx.ALL, 5 )

        self.m_collapsiblePane2 = wx.CollapsiblePane( self, wx.ID_ANY, _(u"Source Files"), wx.DefaultPosition, wx.Size( -1,-1 ), wx.CP_DEFAULT_STYLE|wx.CP_NO_TLW_RESIZE )
        self.m_collapsiblePane2.Collapse( True )

        bSizer2 = wx.BoxSizer( wx.VERTICAL )

        self.open_sourcefiles_button = wx.Button( self.m_collapsiblePane2.GetPane(), wx.ID_ANY, _(u"Open Source Files"), wx.DefaultPosition, wx.DefaultSize, 0 )
        bSizer2.Add( self.open_sourcefiles_button, 0, wx.ALL, 5 )

        self.sourcefile_notebook = wx.Notebook( self.m_collapsiblePane2.GetPane(), wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, 0 )

        bSizer2.Add( self.sourcefile_notebook, 5, wx.EXPAND |wx.ALL, 5 )


        self.m_collapsiblePane2.GetPane().SetSizer( bSizer2 )
        self.m_collapsiblePane2.GetPane().Layout()
        bSizer2.Fit( self.m_collapsiblePane2.GetPane() )
        gbSizer1.Add( self.m_collapsiblePane2, wx.GBPosition( 4, 0 ), wx.GBSpan( 1, 4 ), wx.ALL|wx.EXPAND, 5 )

        self.m_staticline3 = wx.StaticLine( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.LI_HORIZONTAL )
        gbSizer1.Add( self.m_staticline3, wx.GBPosition( 5, 0 ), wx.GBSpan( 1, 4 ), wx.EXPAND |wx.ALL, 5 )

        self.m_staticText3 = wx.StaticText( self, wx.ID_ANY, _(u"Image"), wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText3.Wrap( -1 )

        gbSizer1.Add( self.m_staticText3, wx.GBPosition( 6, 0 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_notebook2 = wx.Notebook( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_panel1 = wx.Panel( self.m_notebook2, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
        gbSizer2 = wx.GridBagSizer( 0, 0 )
        gbSizer2.SetFlexibleDirection( wx.BOTH )
        gbSizer2.SetNonFlexibleGrowMode( wx.FLEX_GROWMODE_SPECIFIED )

        self.m_staticText8 = wx.StaticText( self.m_panel1, wx.ID_ANY, _(u"Max Y"), wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText8.Wrap( -1 )

        gbSizer2.Add( self.m_staticText8, wx.GBPosition( 1, 2 ), wx.GBSpan( 1, 1 ), wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT|wx.ALL, 5 )

        bounding_box_type_choiceChoices = [ _(u"Lat Long"), _(u"Coord") ]
        self.bounding_box_type_choice = wx.Choice( self.m_panel1, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, bounding_box_type_choiceChoices, 0 )
        self.bounding_box_type_choice.SetSelection( 0 )
        gbSizer2.Add( self.bounding_box_type_choice, wx.GBPosition( 0, 1 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.bounding_box_maxy_text_entry = wx.TextCtrl( self.m_panel1, wx.ID_ANY, _(u"51.48589"), wx.DefaultPosition, wx.DefaultSize, wx.TE_RIGHT )
        gbSizer2.Add( self.bounding_box_maxy_text_entry, wx.GBPosition( 1, 3 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_staticText9 = wx.StaticText( self.m_panel1, wx.ID_ANY, _(u"Min X"), wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText9.Wrap( -1 )

        gbSizer2.Add( self.m_staticText9, wx.GBPosition( 2, 0 ), wx.GBSpan( 1, 1 ), wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT|wx.ALL, 5 )

        self.bounding_box_minx_text_entry = wx.TextCtrl( self.m_panel1, wx.ID_ANY, _(u"-0.01710"), wx.DefaultPosition, wx.DefaultSize, wx.TE_RIGHT )
        gbSizer2.Add( self.bounding_box_minx_text_entry, wx.GBPosition( 2, 1 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_staticText10 = wx.StaticText( self.m_panel1, wx.ID_ANY, _(u"Max X"), wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText10.Wrap( -1 )

        gbSizer2.Add( self.m_staticText10, wx.GBPosition( 2, 4 ), wx.GBSpan( 1, 1 ), wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT|wx.ALL, 5 )

        self.bounding_box_maxx_text_entry = wx.TextCtrl( self.m_panel1, wx.ID_ANY, _(u"0.01202"), wx.DefaultPosition, wx.DefaultSize, wx.TE_RIGHT )
        gbSizer2.Add( self.bounding_box_maxx_text_entry, wx.GBPosition( 2, 5 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_staticText11 = wx.StaticText( self.m_panel1, wx.ID_ANY, _(u"Min Y"), wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText11.Wrap( -1 )

        gbSizer2.Add( self.m_staticText11, wx.GBPosition( 3, 2 ), wx.GBSpan( 1, 1 ), wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT|wx.ALL, 5 )

        self.bounding_box_miny_text_entry = wx.TextCtrl( self.m_panel1, wx.ID_ANY, _(u"51.47036"), wx.DefaultPosition, wx.DefaultSize, wx.TE_RIGHT )
        gbSizer2.Add( self.bounding_box_miny_text_entry, wx.GBPosition( 3, 3 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        bounding_box_scale_choiceChoices = [ _(u"Scale"), _(u"Zoom") ]
        self.bounding_box_scale_choice = wx.Choice( self.m_panel1, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, bounding_box_scale_choiceChoices, 0 )
        self.bounding_box_scale_choice.SetSelection( 0 )
        gbSizer2.Add( self.bounding_box_scale_choice, wx.GBPosition( 4, 0 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.bounding_box_scale_text_entry = wx.TextCtrl( self.m_panel1, wx.ID_ANY, _(u"10"), wx.DefaultPosition, wx.DefaultSize, wx.TE_RIGHT )
        gbSizer2.Add( self.bounding_box_scale_text_entry, wx.GBPosition( 4, 1 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_staticText19 = wx.StaticText( self.m_panel1, wx.ID_ANY, _(u"Type"), wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText19.Wrap( -1 )

        gbSizer2.Add( self.m_staticText19, wx.GBPosition( 0, 0 ), wx.GBSpan( 1, 1 ), wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )


        self.m_panel1.SetSizer( gbSizer2 )
        self.m_panel1.Layout()
        gbSizer2.Fit( self.m_panel1 )
        self.m_notebook2.AddPage( self.m_panel1, _(u"Bounding Box"), True )
        self.m_panel2 = wx.Panel( self.m_notebook2, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
        gbSizer3 = wx.GridBagSizer( 0, 0 )
        gbSizer3.SetFlexibleDirection( wx.BOTH )
        gbSizer3.SetNonFlexibleGrowMode( wx.FLEX_GROWMODE_SPECIFIED )

        self.m_staticText13 = wx.StaticText( self.m_panel2, wx.ID_ANY, _(u"Type"), wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText13.Wrap( -1 )

        gbSizer3.Add( self.m_staticText13, wx.GBPosition( 0, 0 ), wx.GBSpan( 1, 1 ), wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )

        center_type_choiceChoices = [ _(u"Lat Long"), _(u"Coord") ]
        self.center_type_choice = wx.Choice( self.m_panel2, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, center_type_choiceChoices, 0 )
        self.center_type_choice.SetSelection( 0 )
        gbSizer3.Add( self.center_type_choice, wx.GBPosition( 0, 1 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.center_xcoord_label = wx.StaticText( self.m_panel2, wx.ID_ANY, _(u"Latitude"), wx.DefaultPosition, wx.DefaultSize, 0 )
        self.center_xcoord_label.Wrap( -1 )

        gbSizer3.Add( self.center_xcoord_label, wx.GBPosition( 1, 0 ), wx.GBSpan( 1, 1 ), wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )

        self.center_xcoord_text_entry = wx.TextCtrl( self.m_panel2, wx.ID_ANY, _(u"51.47629"), wx.DefaultPosition, wx.DefaultSize, wx.TE_RIGHT )
        self.center_xcoord_text_entry.SetMaxLength( 10 )
        gbSizer3.Add( self.center_xcoord_text_entry, wx.GBPosition( 1, 1 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.center_ycoord_label = wx.StaticText( self.m_panel2, wx.ID_ANY, _(u"Longnitude"), wx.DefaultPosition, wx.DefaultSize, 0 )
        self.center_ycoord_label.Wrap( -1 )

        gbSizer3.Add( self.center_ycoord_label, wx.GBPosition( 1, 2 ), wx.GBSpan( 1, 1 ), wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )

        self.center_ycoord_text_entry = wx.TextCtrl( self.m_panel2, wx.ID_ANY, _(u"0.00034"), wx.DefaultPosition, wx.DefaultSize, wx.TE_RIGHT )
        self.center_ycoord_text_entry.SetMaxLength( 10 )
        gbSizer3.Add( self.center_ycoord_text_entry, wx.GBPosition( 1, 3 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        center_scale_choiceChoices = [ _(u"Scale"), _(u"Zoom") ]
        self.center_scale_choice = wx.Choice( self.m_panel2, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, center_scale_choiceChoices, 0 )
        self.center_scale_choice.SetSelection( 0 )
        gbSizer3.Add( self.center_scale_choice, wx.GBPosition( 2, 0 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.center_scale_text_entry = wx.TextCtrl( self.m_panel2, wx.ID_ANY, _(u"10"), wx.DefaultPosition, wx.DefaultSize, wx.TE_RIGHT )
        gbSizer3.Add( self.center_scale_text_entry, wx.GBPosition( 2, 1 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_staticText17 = wx.StaticText( self.m_panel2, wx.ID_ANY, _(u"Width"), wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText17.Wrap( -1 )

        gbSizer3.Add( self.m_staticText17, wx.GBPosition( 3, 0 ), wx.GBSpan( 1, 1 ), wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )

        self.m_textCtrl14 = wx.TextCtrl( self.m_panel2, wx.ID_ANY, _(u"500"), wx.DefaultPosition, wx.DefaultSize, wx.TE_RIGHT )
        gbSizer3.Add( self.m_textCtrl14, wx.GBPosition( 3, 1 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_staticText18 = wx.StaticText( self.m_panel2, wx.ID_ANY, _(u"Height"), wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText18.Wrap( -1 )

        gbSizer3.Add( self.m_staticText18, wx.GBPosition( 3, 2 ), wx.GBSpan( 1, 1 ), wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )

        self.height_text_entry = wx.TextCtrl( self.m_panel2, wx.ID_ANY, _(u"500"), wx.DefaultPosition, wx.DefaultSize, wx.TE_RIGHT )
        gbSizer3.Add( self.height_text_entry, wx.GBPosition( 3, 3 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )


        self.m_panel2.SetSizer( gbSizer3 )
        self.m_panel2.Layout()
        gbSizer3.Fit( self.m_panel2 )
        self.m_notebook2.AddPage( self.m_panel2, _(u"Center"), False )
        self.m_panel3 = wx.Panel( self.m_notebook2, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
        gbSizer4 = wx.GridBagSizer( 0, 0 )
        gbSizer4.SetFlexibleDirection( wx.BOTH )
        gbSizer4.SetNonFlexibleGrowMode( wx.FLEX_GROWMODE_SPECIFIED )


        self.m_panel3.SetSizer( gbSizer4 )
        self.m_panel3.Layout()
        gbSizer4.Fit( self.m_panel3 )
        self.m_notebook2.AddPage( self.m_panel3, _(u"Tile"), False )

        gbSizer1.Add( self.m_notebook2, wx.GBPosition( 6, 1 ), wx.GBSpan( 1, 1 ), wx.EXPAND |wx.ALL, 5 )


        gbSizer1.AddGrowableCol( 3 )

        self.SetSizer( gbSizer1 )
        self.Layout()

        self.Centre( wx.BOTH )

    def __del__( self ):
        pass


###########################################################################
## Class SourceFileBase
###########################################################################

class SourceFileBase ( wx.Panel ):

    def __init__( self, parent, id = wx.ID_ANY, pos = wx.DefaultPosition, size = wx.Size( -1,-1 ), style = wx.TAB_TRAVERSAL, name = wx.EmptyString ):
        wx.Panel.__init__ ( self, parent, id = id, pos = pos, size = size, style = style, name = name )

        bSizer1 = wx.BoxSizer( wx.VERTICAL )

        self.scintilla = wx.stc.StyledTextCtrl( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, 0)
        self.scintilla.SetUseTabs ( True )
        self.scintilla.SetTabWidth ( 4 )
        self.scintilla.SetIndent ( 4 )
        self.scintilla.SetTabIndents( True )
        self.scintilla.SetBackSpaceUnIndents( True )
        self.scintilla.SetViewEOL( False )
        self.scintilla.SetViewWhiteSpace( False )
        self.scintilla.SetMarginWidth( 2, 0 )
        self.scintilla.SetIndentationGuides( True )
        self.scintilla.SetReadOnly( False )
        self.scintilla.SetMarginType ( 1, wx.stc.STC_MARGIN_SYMBOL )
        self.scintilla.SetMarginMask ( 1, wx.stc.STC_MASK_FOLDERS )
        self.scintilla.SetMarginWidth ( 1, 16)
        self.scintilla.SetMarginSensitive( 1, True )
        self.scintilla.SetProperty ( "fold", "1" )
        self.scintilla.SetFoldFlags ( wx.stc.STC_FOLDFLAG_LINEBEFORE_CONTRACTED | wx.stc.STC_FOLDFLAG_LINEAFTER_CONTRACTED )
        self.scintilla.SetMarginType( 0, wx.stc.STC_MARGIN_NUMBER )
        self.scintilla.SetMarginWidth( 0, self.scintilla.TextWidth( wx.stc.STC_STYLE_LINENUMBER, "_99999" ) )
        self.scintilla.MarkerDefine( wx.stc.STC_MARKNUM_FOLDER, wx.stc.STC_MARK_BOXPLUS )
        self.scintilla.MarkerSetBackground( wx.stc.STC_MARKNUM_FOLDER, wx.BLACK)
        self.scintilla.MarkerSetForeground( wx.stc.STC_MARKNUM_FOLDER, wx.WHITE)
        self.scintilla.MarkerDefine( wx.stc.STC_MARKNUM_FOLDEROPEN, wx.stc.STC_MARK_BOXMINUS )
        self.scintilla.MarkerSetBackground( wx.stc.STC_MARKNUM_FOLDEROPEN, wx.BLACK )
        self.scintilla.MarkerSetForeground( wx.stc.STC_MARKNUM_FOLDEROPEN, wx.WHITE )
        self.scintilla.MarkerDefine( wx.stc.STC_MARKNUM_FOLDERSUB, wx.stc.STC_MARK_EMPTY )
        self.scintilla.MarkerDefine( wx.stc.STC_MARKNUM_FOLDEREND, wx.stc.STC_MARK_BOXPLUS )
        self.scintilla.MarkerSetBackground( wx.stc.STC_MARKNUM_FOLDEREND, wx.BLACK )
        self.scintilla.MarkerSetForeground( wx.stc.STC_MARKNUM_FOLDEREND, wx.WHITE )
        self.scintilla.MarkerDefine( wx.stc.STC_MARKNUM_FOLDEROPENMID, wx.stc.STC_MARK_BOXMINUS )
        self.scintilla.MarkerSetBackground( wx.stc.STC_MARKNUM_FOLDEROPENMID, wx.BLACK)
        self.scintilla.MarkerSetForeground( wx.stc.STC_MARKNUM_FOLDEROPENMID, wx.WHITE)
        self.scintilla.MarkerDefine( wx.stc.STC_MARKNUM_FOLDERMIDTAIL, wx.stc.STC_MARK_EMPTY )
        self.scintilla.MarkerDefine( wx.stc.STC_MARKNUM_FOLDERTAIL, wx.stc.STC_MARK_EMPTY )
        self.scintilla.SetSelBackground( True, wx.SystemSettings.GetColour(wx.SYS_COLOUR_HIGHLIGHT ) )
        self.scintilla.SetSelForeground( True, wx.SystemSettings.GetColour(wx.SYS_COLOUR_HIGHLIGHTTEXT ) )
        self.scintilla.SetMinSize( wx.Size( 500,300 ) )

        bSizer1.Add( self.scintilla, 1, wx.EXPAND |wx.ALL, 5 )


        self.SetSizer( bSizer1 )
        self.Layout()
        bSizer1.Fit( self )

    def __del__( self ):
        pass


